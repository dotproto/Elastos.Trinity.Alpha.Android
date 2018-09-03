# Elastos.Trinity.Android
# 提交google chromium代码到github
- 需要先删除各个第三方的.git仓库
- 删除.gitignore
- 将从google拉取的chromium代码提交到git@github.com:elastos/Elastos.Trinity.Android.git
- 将之前删除.gitignore的文件重新合入（主要是.gitignore忽略了依赖第三方库的代码目录，但同时许多生成的文件，我们确实是不用提交的）

# 编译github上的代码

# elastos_web_rt 说明
## 下载代码

```
$git clone https://github.com/elastos/Elastos.Trinity.android trinty
```

## 配置参数

```
$gn args out/arm
```

按照下面参数配置

```
target_os="android"
target_cpu = "arm"
is_debug=false
symbol_level=1
is_official_build=true
is_chrome_branded=false
use_official_google_api_keys=false
enable_resource_whitelist_generation=false
ffmpeg_branding="Chrome"
proprietary_codecs=true
enable_remoting=true
```

## 编译

```
$ninja -C out/arm elastos_webrt_apk
```

生成的文件，我们需要两个：
Elastos.Trinity.android/src/out/arm/apks/ElastosWebRT.apk和Elastos.Trinity.android/src/out/arm/lib.java/elastos_webrt_java.jar

## 处理目标文件
由于我们需要把jar包和so提供给studio环境使用，而chromium构建中有部分代码是在生成apk时才自动填充，所以我们分离出的jar包目前需要手动替换，请按下面的步骤进行操作：

    a. 手动替换elastos_webrt_java.jar中nativelibrary，buildconfig目录下的class文件。
    用Elastos.Trinity.android/src/out/arm/gen/content/cordova/android/elastos_webrt_apk/elastos_webrt_apk.jar里的/org/chromium/base/
    替换Elastos.Trinity.android/src/out/arm/lib.java/elastos_webrt_java.jar里的/org/chromium/base/
    b. 删除elastos_webrt_java.jar中与AndroidStudio冲突的package(com.android.support)，目录如下：
    elastos_webrt_java.jar/android/support/v4

## ElastosWebRT 输出主要包含如下文件：
    elastos_webrt_java.jar(chromium java 层代码生成的jar包)
    cordovaLib (cordava 库代码)
    asset(dat，bin，pak即chromium相关资源文件)
    Build.gradle(diff)
    manifest.xml(diff)
    android.jar
使用方式有点复杂，但如果是基于Elastos.ORG.Wallet开发并不需要做这些复杂的操作，只需要clone 下来，替换一下sdk里的android.jar包就可以了。只有开发web engine的人员才需要处理。

由于代码量巨大，git clone经常会失败，所以请使用git fetch下载代码：

```
git init
git fetch https://github.com/elastos/Elastos.Trinity.Android.git
git checkout FETCH_HEAD
```

61e373a677a42fcdc951221ba2d0d16d13c738a8
