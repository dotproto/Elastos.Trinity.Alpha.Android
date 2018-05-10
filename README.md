# Elastos.Trinity.Android
# 提交google chromium代码到github
- 需要先删除各个第三方的.git仓库
- 删除.gitignore
- 将从google拉取的chromium代码提交到git@github.com:elastos/Elastos.Trinity.Android.git
- 将之前删除.gitignore的文件重新合入（主要是.gitignore忽略了依赖第三方库的代码目录，但同时许多生成的文件，我们确实是不用提交的）

# 编译github上的代码

```
git clone https://github.com/elastos/Elastos.Trinity.Android.git elastos
cd elastos/src
export PATH="your_dir/elastos/depot_tools$PATH"
gn args out/chrome
```
gn 配置参数如下：
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

开始编译
```
ninja -C out/chrome chrome_public_apk
```

由于代码量巨大，git clone经常会失败，所以请使用git fetch下载代码：

```
git init
git fetch https://github.com/elastos/Elastos.Trinity.Android.git
git checkout FETCH_HEAD
```
