// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ios/chrome/browser/ui/authentication/chrome_signin_view_controller.h"

#include "base/memory/ptr_util.h"
#import "base/strings/sys_string_conversions.h"
#import "base/test/ios/wait_util.h"
#include "base/test/scoped_feature_list.h"
#include "base/timer/mock_timer.h"
#include "components/consent_auditor/consent_auditor.h"
#include "components/signin/core/browser/account_tracker_service.h"
#include "components/signin/core/browser/profile_management_switches.h"
#include "components/version_info/version_info.h"
#include "ios/chrome/browser/application_context.h"
#include "ios/chrome/browser/browser_state/test_chrome_browser_state.h"
#include "ios/chrome/browser/consent_auditor/consent_auditor_factory.h"
#include "ios/chrome/browser/signin/account_tracker_service_factory.h"
#import "ios/chrome/browser/signin/authentication_service_factory.h"
#import "ios/chrome/browser/signin/authentication_service_fake.h"
#include "ios/chrome/browser/sync/ios_user_event_service_factory.h"
#include "ios/chrome/grit/ios_chromium_strings.h"
#include "ios/chrome/grit/ios_strings.h"
#import "ios/public/provider/chrome/browser/signin/fake_chrome_identity.h"
#import "ios/public/provider/chrome/browser/signin/fake_chrome_identity_service.h"
#import "ios/testing/wait_util.h"
#include "ios/web/public/test/test_web_thread_bundle.h"
#import "testing/platform_test.h"
#import "third_party/ocmock/OCMock/OCMock.h"
#include "third_party/ocmock/gtest_support.h"
#include "ui/base/l10n/l10n_util_mac.h"

#if !defined(__has_feature) || !__has_feature(objc_arc)
#error "This file requires ARC support."
#endif

@interface FakeChromeSigninViewControllerDelegate
    : NSObject<ChromeSigninViewControllerDelegate>

@property(nonatomic) BOOL didSigninCalled;
@end

@implementation FakeChromeSigninViewControllerDelegate

@synthesize didSigninCalled = _didSigninCalled;

- (void)willStartSignIn:(ChromeSigninViewController*)controller {
}

- (void)willStartAddAccount:(ChromeSigninViewController*)controller {
}

- (void)didSkipSignIn:(ChromeSigninViewController*)controller {
}

- (void)didFailSignIn:(ChromeSigninViewController*)controller {
  FAIL();
}

- (void)didSignIn:(ChromeSigninViewController*)controller {
  ASSERT_FALSE(self.didSigninCalled);
  self.didSigninCalled = YES;
}

- (void)didUndoSignIn:(ChromeSigninViewController*)controller
             identity:(ChromeIdentity*)identity {
}

- (void)didAcceptSignIn:(ChromeSigninViewController*)controller
    showAccountsSettings:(BOOL)showAccountsSettings {
}

@end

namespace {

const bool kUnifiedConsentParam[] = {
    false, true,
};

// Fake consent auditor used for the tests.
class FakeConsentAuditor : public consent_auditor::ConsentAuditor {
 public:
  static std::unique_ptr<KeyedService> CreateInstance(
      web::BrowserState* context) {
    ios::ChromeBrowserState* ios_context =
        ios::ChromeBrowserState::FromBrowserState(context);
    syncer::UserEventService* const user_event_service =
        IOSUserEventServiceFactory::GetForBrowserState(ios_context);
    return std::make_unique<FakeConsentAuditor>(
        ios_context->GetPrefs(), user_event_service,
        version_info::GetVersionNumber(),
        GetApplicationContext()->GetApplicationLocale());
  }

  FakeConsentAuditor(PrefService* pref_service,
                     syncer::UserEventService* user_event_service,
                     const std::string& app_version,
                     const std::string& app_locale)
      : ConsentAuditor(pref_service,
                       user_event_service,
                       app_version,
                       app_locale) {}
  ~FakeConsentAuditor() override {}

  void RecordGaiaConsent(const std::string& account_id,
                         consent_auditor::Feature feature,
                         const std::vector<int>& description_grd_ids,
                         int confirmation_string_id,
                         consent_auditor::ConsentStatus status) override {
    account_id_ = account_id;
    feature_ = feature;
    recorded_ids_ = description_grd_ids;
    confirmation_string_id_ = confirmation_string_id;
    status_ = status;
  }

  const std::string& account_id() const { return account_id_; }
  consent_auditor::Feature feature() const { return feature_; }
  const std::vector<int>& recorded_ids() const { return recorded_ids_; }
  int confirmation_string_id() const { return confirmation_string_id_; }
  consent_auditor::ConsentStatus status() const { return status_; }

 private:
  std::string account_id_;
  consent_auditor::Feature feature_;
  std::vector<int> recorded_ids_;
  int confirmation_string_id_ = -1;
  consent_auditor::ConsentStatus status_;

  DISALLOW_COPY_AND_ASSIGN(FakeConsentAuditor);
};

// These tests verify that Chrome correctly records user's consent to Chrome
// Sync, which is a GDPR requirement. None of those tests should be turned off.
// If one of those tests fails, one of the following methods should be updated
// with the added or removed strings:
//   - ExpectedConsentStringIds()
//   - WhiteListLocalizedStrings()
class ChromeSigninViewControllerTest
    : public PlatformTest,
      public ::testing::WithParamInterface<bool> {
 protected:
  void SetUp() override {
    PlatformTest::SetUp();
    if (GetParam()) {
      unified_consent_enabled_ = true;
      scoped_feature_list_.InitAndEnableFeature(signin::kUnifiedConsent);
    } else {
      unified_consent_enabled_ = false;
    }
    identity_ = [FakeChromeIdentity identityWithEmail:@"foo1@gmail.com"
                                               gaiaID:@"foo1ID"
                                                 name:@"Fake Foo 1"];
    // Setup services.
    TestChromeBrowserState::Builder builder;
    builder.AddTestingFactory(
        AuthenticationServiceFactory::GetInstance(),
        AuthenticationServiceFake::CreateAuthenticationService);
    builder.AddTestingFactory(ConsentAuditorFactory::GetInstance(),
                              FakeConsentAuditor::CreateInstance);
    context_ = builder.Build();
    ios::FakeChromeIdentityService* identity_service =
        ios::FakeChromeIdentityService::GetInstanceFromChromeProvider();
    identity_service->AddIdentity(identity_);
    account_tracker_service_ =
        ios::AccountTrackerServiceFactory::GetForBrowserState(context_.get());
    fake_consent_auditor_ = static_cast<FakeConsentAuditor*>(
        ConsentAuditorFactory::GetForBrowserState(context_.get()));
    // Setup view controller.
    vc_ = [[ChromeSigninViewController alloc]
        initWithBrowserState:context_.get()
                 accessPoint:signin_metrics::AccessPoint::ACCESS_POINT_SETTINGS
                 promoAction:signin_metrics::PromoAction::
                                 PROMO_ACTION_WITH_DEFAULT
              signInIdentity:identity_
                  dispatcher:nil];
    vc_delegate_ = [[FakeChromeSigninViewControllerDelegate alloc] init];
    vc_.delegate = vc_delegate_;
    __block base::MockTimer* mock_timer_ptr = nullptr;
    if (!unified_consent_enabled_) {
      vc_.timerGenerator = ^std::unique_ptr<base::Timer>(bool retain_user_task,
                                                         bool is_repeating) {
        auto mock_timer =
            std::make_unique<base::MockTimer>(retain_user_task, is_repeating);
        mock_timer_ptr = mock_timer.get();
        return mock_timer;
      };
    }
    UIScreen* screen = [UIScreen mainScreen];
    UIWindow* window = [[UIWindow alloc] initWithFrame:screen.bounds];
    [window makeKeyAndVisible];
    [window addSubview:[vc_ view]];
    if (!unified_consent_enabled_) {
      ASSERT_TRUE(mock_timer_ptr);
      mock_timer_ptr->Fire();
    }
    window_ = window;
  }

  // Adds in |string_set|, all the strings displayed by |view| and its subviews,
  // recursively.
  static void AddStringsFromView(NSMutableSet<NSString*>* string_set,
                                 UIView* view) {
    for (UIView* subview in view.subviews)
      AddStringsFromView(string_set, subview);
    if ([view isKindOfClass:[UIButton class]]) {
      UIButton* button = static_cast<UIButton*>(view);
      if (button.currentTitle)
        [string_set addObject:button.currentTitle];
    } else if ([view isKindOfClass:[UILabel class]]) {
      UILabel* label = static_cast<UILabel*>(view);
      if (label.text)
        [string_set addObject:label.text];
    } else {
      NSString* view_name = NSStringFromClass([view class]);
      // Views that don't display strings.
      NSArray* other_views = @[
        @"AccountControlCell", @"CollectionViewFooterCell", @"UIButtonLabel",
        @"UICollectionView", @"UICollectionViewControllerWrapperView",
        @"UIImageView", @"UIView", @"MDCActivityIndicator", @"MDCButtonBar",
        @"MDCFlexibleHeaderView", @"MDCHeaderStackView", @"MDCInkView",
        @"MDCNavigationBar", @"UIScrollView", @"IdentityPickerView"
      ];
      // If this test fails, the unknown class should be added in other_views if
      // it doesn't display any strings, otherwise the strings diplay by this
      // class should be added in string_set.
      EXPECT_TRUE([other_views containsObject:view_name])
          << base::SysNSStringToUTF8(view_name);
    }
  }

  // Returns the set of strings displayed on the screen based on the views
  // displayed by the .
  NSSet<NSString*>* LocalizedStringOnScreen() const {
    NSMutableSet* string_set = [NSMutableSet set];
    AddStringsFromView(string_set, vc_.view);
    return string_set;
  }

  // Returns a localized string based on the string id.
  NSString* LocalizedStringFromID(int string_id) const {
    NSString* string = l10n_util::GetNSString(string_id);
    string = [string stringByReplacingOccurrencesOfString:@"BEGIN_LINK"
                                               withString:@""];
    string = [string stringByReplacingOccurrencesOfString:@"END_LINK"
                                               withString:@""];
    return string;
  }

  // Returns all the strings on screen that should be part of the user consent
  // and part of the white list strings.
  NSSet<NSString*>* LocalizedExpectedStringsOnScreen() const {
    const std::vector<int>& string_ids = ExpectedConsentStringIds();
    NSMutableSet<NSString*>* set = [NSMutableSet set];
    for (auto it = string_ids.begin(); it != string_ids.end(); ++it) {
      [set addObject:LocalizedStringFromID(*it)];
    }
    [set unionSet:WhiteListLocalizedStrings()];
    return set;
  }

  // Returns the list of string id that should be given to RecordGaiaConsent()
  // then the consent is given. The list is ordered according to the position
  // on the screen.
  const std::vector<int> ExpectedConsentStringIds() const {
    if (unified_consent_enabled_) {
      return {
          IDS_IOS_ACCOUNT_UNIFIED_CONSENT_TITLE,
          IDS_IOS_ACCOUNT_UNIFIED_CONSENT_SYNC_DATA,
          IDS_IOS_ACCOUNT_UNIFIED_CONSENT_PERSONALIZED,
          IDS_IOS_ACCOUNT_UNIFIED_CONSENT_BETTER_BROWSER,
          IDS_IOS_ACCOUNT_UNIFIED_CONSENT_SETTINGS,
      };
    }
    return {
        IDS_IOS_ACCOUNT_CONSISTENCY_CONFIRMATION_SYNC_TITLE,
        IDS_IOS_ACCOUNT_CONSISTENCY_CONFIRMATION_SYNC_DESCRIPTION,
        IDS_IOS_ACCOUNT_CONSISTENCY_CONFIRMATION_SERVICES_TITLE,
        IDS_IOS_ACCOUNT_CONSISTENCY_CONFIRMATION_SERVICES_DESCRIPTION,
        IDS_IOS_ACCOUNT_CONSISTENCY_CONFIRMATION_OPEN_SETTINGS,
    };
  }

  // Returns the white list of strings that can be displayed on screen but
  // should not be part of ExpectedConsentStringIds().
  NSSet<NSString*>* WhiteListLocalizedStrings() const {
    if (unified_consent_enabled_) {
      return [NSSet setWithObjects:@"Fake Foo 1", @"foo1@gmail.com", @"CANCEL",
                                   @"YES, I'M IN", nil];
    }
    return [NSSet setWithObjects:@"Hi, Fake Foo 1", @"foo1@gmail.com",
                                 @"OK, GOT IT", @"UNDO", nil];
  }

  int ConfirmationStringId() const {
    if (unified_consent_enabled_) {
      return IDS_IOS_ACCOUNT_UNIFIED_CONSENT_OK_BUTTON;
    }
    return IDS_IOS_ACCOUNT_CONSISTENCY_CONFIRMATION_OK_BUTTON;
  }

  int SettingsConfirmationStringId() const {
    if (unified_consent_enabled_) {
      return IDS_IOS_ACCOUNT_UNIFIED_CONSENT_SETTINGS;
    }
    return IDS_IOS_ACCOUNT_CONSISTENCY_CONFIRMATION_OPEN_SETTINGS;
  }

  bool IsMoreButtonVisible() {
    UIButton* primary_button = vc_.primaryButton;
    NSString* primary_title = primary_button.currentTitle;
    BOOL is_more_button =
        [primary_title
            caseInsensitiveCompare:
                l10n_util::GetNSString(
                    IDS_IOS_ACCOUNT_CONSISTENCY_CONFIRMATION_SCROLL_BUTTON)] ==
        NSOrderedSame;
    return !primary_button.isHidden && is_more_button;
  }

  // Waits until all expected strings are on the screen.
  void WaitAndExpectAllStringsOnScreen() {
    __block NSSet<NSString*>* notFoundStrings = nil;
    __block NSSet<NSString*>* notExpectedStrings = nil;
    ConditionBlock condition = ^bool() {
      if (IsMoreButtonVisible()) {
        [vc_.primaryButton
            sendActionsForControlEvents:UIControlEventTouchUpInside];
      }
      NSSet<NSString*>* foundStrings = LocalizedStringOnScreen();
      NSSet<NSString*>* expectedStrings = LocalizedExpectedStringsOnScreen();
      notFoundStrings = [expectedStrings
          objectsPassingTest:^BOOL(NSString* string, BOOL* stop) {
            return ![foundStrings containsObject:string];
          }];
      notExpectedStrings =
          [foundStrings objectsPassingTest:^BOOL(NSString* string, BOOL* stop) {
            return ![expectedStrings containsObject:string];
          }];
      return [foundStrings isEqual:expectedStrings];
    };
    bool conditionMet = testing::WaitUntilConditionOrTimeout(10, condition);
    NSString* failureExplaination = [NSString
        stringWithFormat:@"Strings not found: %@, Strings not expected: %@",
                         notFoundStrings, notExpectedStrings];
    EXPECT_TRUE(conditionMet) << base::SysNSStringToUTF8(failureExplaination);
  }

  bool unified_consent_enabled_;
  web::TestWebThreadBundle thread_bundle_;
  std::unique_ptr<TestChromeBrowserState> context_;
  FakeChromeIdentity* identity_;
  UIWindow* window_;
  ChromeSigninViewController* vc_;
  FakeConsentAuditor* fake_consent_auditor_;
  AccountTrackerService* account_tracker_service_;
  base::test::ScopedFeatureList scoped_feature_list_;
  base::MockTimer* mock_timer_ptr_ = nullptr;
  FakeChromeSigninViewControllerDelegate* vc_delegate_;
};

INSTANTIATE_TEST_CASE_P(,
                        ChromeSigninViewControllerTest,
                        ::testing::ValuesIn(kUnifiedConsentParam));

// Tests that all strings on the screen are either part of the consent string
// list defined in FakeConsentAuditor::ExpectedConsentStringIds()), or are part
// of the white list strings defined in
// FakeConsentAuditor::WhiteListLocalizedStrings().
// TODO(crbug.com/839001): Reenable this test.
TEST_F(ChromeSigninViewControllerTest, DISABLED_TestAllStrings) {
  WaitAndExpectAllStringsOnScreen();
}

// Tests when the user taps on "OK GOT IT", that RecordGaiaConsent() is called
// with the expected list of string ids, and confirmation string id.
// TODO(crbug.com/839001): Reenable this test.
TEST_F(ChromeSigninViewControllerTest, DISABLED_TestConsentWithOKGOTIT) {
  WaitAndExpectAllStringsOnScreen();
  [vc_.primaryButton sendActionsForControlEvents:UIControlEventTouchUpInside];
  ConditionBlock condition = ^bool() {
    return this->vc_delegate_.didSigninCalled;
  };
  EXPECT_TRUE(testing::WaitUntilConditionOrTimeout(10, condition));
  const std::vector<int>& recorded_ids = fake_consent_auditor_->recorded_ids();
  EXPECT_EQ(ExpectedConsentStringIds(), recorded_ids);
  EXPECT_EQ(ConfirmationStringId(),
            fake_consent_auditor_->confirmation_string_id());
  EXPECT_EQ(consent_auditor::ConsentStatus::GIVEN,
            fake_consent_auditor_->status());
  EXPECT_EQ(consent_auditor::Feature::CHROME_SYNC,
            fake_consent_auditor_->feature());
  EXPECT_EQ(account_tracker_service_->PickAccountIdForAccount(
                base::SysNSStringToUTF8([identity_ gaiaID]),
                base::SysNSStringToUTF8([identity_ userEmail])),
            fake_consent_auditor_->account_id());
}

// Tests that RecordGaiaConsent() is not called when the user taps on UNDO.
// TODO(crbug.com/839001): Reenable this test.
TEST_F(ChromeSigninViewControllerTest, DISABLED_TestRefusingConsent) {
  WaitAndExpectAllStringsOnScreen();
  [vc_.secondaryButton sendActionsForControlEvents:UIControlEventTouchUpInside];
  const std::vector<int>& recorded_ids = fake_consent_auditor_->recorded_ids();
  EXPECT_EQ(0ul, recorded_ids.size());
  EXPECT_EQ(-1, fake_consent_auditor_->confirmation_string_id());
}

// Tests that RecordGaiaConsent() is called with the expected list of string
// ids, and settings confirmation string id.
// TODO(crbug.com/839001): Reenable this test.
TEST_F(ChromeSigninViewControllerTest, DISABLED_TestConsentWithSettings) {
  WaitAndExpectAllStringsOnScreen();
  [vc_ signinConfirmationControllerDidTapSettingsLink:vc_.confirmationVC];
  const std::vector<int>& recorded_ids = fake_consent_auditor_->recorded_ids();
  EXPECT_EQ(ExpectedConsentStringIds(), recorded_ids);
  EXPECT_EQ(SettingsConfirmationStringId(),
            fake_consent_auditor_->confirmation_string_id());
  EXPECT_EQ(consent_auditor::ConsentStatus::GIVEN,
            fake_consent_auditor_->status());
  EXPECT_EQ(consent_auditor::Feature::CHROME_SYNC,
            fake_consent_auditor_->feature());
  EXPECT_EQ(account_tracker_service_->PickAccountIdForAccount(
                base::SysNSStringToUTF8([identity_ gaiaID]),
                base::SysNSStringToUTF8([identity_ userEmail])),
            fake_consent_auditor_->account_id());
}

}  // namespace
