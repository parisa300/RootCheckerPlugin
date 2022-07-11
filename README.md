# Flutter Plugin For RootCheck
## 2022 developed

A simple plugin to present native rootcheck, 
 various platforms including iOS, Android, Windows,



## Features
## pubspec.yaml

      plugin:
        platforms:
            android:
                package: com.example.pluginforc
                pluginClass: RootCheckerPlugin
            ios:
              pluginClass: RootCheckerPlugin
            windows:
              pluginClass: RootCheckerPlugin
    



## Kotlin
```kotlin
/** RootCheckerPlugin  */
class RootCheckerPlugin :  MethodCallHandler{

    val rootsAppPackage = arrayOf(
        "com.noshufou.android.su",
        "com.noshufou.android.su.elite",
        "eu.chainfire.supersu",
        "com.koushikdutta.superuser",
        "com.thirdparty.superuser",
        "com.yellowes.su",
        "com.topjohnwu.magisk"
    )

    val dangerousListApps = arrayOf(
        "com.koushikdutta.rommanager",
        "com.koushikdutta.rommanager.license",
        "com.dimonvideo.luckypatcher",
        "com.chelpus.lackypatch",
        "com.ramdroid.appquarantine",
        "com.ramdroid.appquarantinepro",
        "com.android.vending.billing.InAppBillingService.COIN",
        "com.chelpus.luckypatcher"
    )

    val rootCloakingApps = arrayOf(
        "com.devadvance.rootcloak",
        "com.devadvance.rootcloakplus",
        "de.robv.android.xposed.installer",
        "com.saurik.substrate",
        "com.zachspong.temprootremovejb",
        "com.amphoras.hidemyroot",
        "com.amphoras.hidemyrootadfree",
        "com.formyhm.hiderootPremium",
        "com.formyhm.hideroot"
    )

    val superUserPath = arrayOf(
        "/data/local/",
        "/data/local/bin/",
        "/data/local/xbin/",
        "/sbin/",
        "/su/bin/",
        "/system/bin/",
        "/system/bin/.ext/",
        "/system/bin/failsafe/",
        "/system/sd/xbin/",
        "/system/usr/we-need-root/",
        "/system/xbin/",
        "/cache",
        "/data",
        "/dev"
    )

    val notWritablePath = arrayOf(
        "/system",
        "/system/bin",
        "/system/sbin",
        "/system/xbin",
        "/vendor/bin",
        "/sbin",
        "/etc"
    )
  override  fun onMethodCall( call: MethodCall, result: Result) {
        if (call.method.equals("isDeviceRooted")) {
            checkHpBodong(result)
        } else {
            result.notImplemented()
        }
    }

    private fun checkHpBodong(result: Result) {
        result.success(
            isPathExist("su")
                    || isSUExist
                    || isHaveDangerousApps
                    || isHaveRootManagementApps
                    || isHaveDangerousProperties
                    || isHaveReadWritePermission
        )
    }

    private fun isPathExist(ext: String): Boolean {
        for (path in superUserPath) {
            val joinPath = path + ext
            val file = File(path, ext)
            if (file.exists()) {
                 Log.e("ROOT_CHECKER", "Path is exist : $joinPath")
                return true
            }
        }
        return false
    }

    private val isSUExist: Boolean
        private get() {
            var process: Process? = null
            return try {
                process = Runtime.getRuntime().exec(arrayOf("/system/xbin/which", "su"))
                val `in` = BufferedReader(InputStreamReader(process.inputStream))
                if (`in`.readLine() != null) {
                       Log.e("ROOT_CHECKER", "cammand executed")
                    return true
                }
                false
            } catch (e: Exception) {
                false
            } finally {
                process?.destroy()
            }
        }
/*    private val isTestBuildKey: Boolean
        private get() {

            val buildTags: String = Build.TAGS
            if (buildTags != null && buildTags.contains("test-keys")) {
                    Log.e("ROOT_CHECKER", "devices buid with test key")
                return true
            }
            return false
        }*/
    private val isHaveDangerousApps: Boolean
        private get() {
            val packages = ArrayList<String>()
            packages.addAll(Arrays.asList<String>(dangerousListApps.toString()))
            return isAnyPackageFromListInstalled(packages)
        }
    private val isHaveRootManagementApps: Boolean
        private get() {
            val packages = ArrayList<String>()
            packages.addAll(Arrays.asList<String>(rootsAppPackage.toString()))
            return isAnyPackageFromListInstalled(packages)
        }

    //check dangerous properties
    private val isHaveDangerousProperties: Boolean
        private get() {
            val dangerousProps: MutableMap<String, String> = HashMap()
            dangerousProps["ro.debuggable"] = "1"
            dangerousProps["ro.secure"] = "0"
            var result = false
            val lines = commander("getprop") ?: return false
            for (line in lines) {
                for (key in dangerousProps.keys) {
                    if (line.contains(key)) {
                        var badValue = dangerousProps[key]
                        badValue = "[$badValue]"
                        if (line.contains(badValue)) {
                            Log.e(
                              "ROOT_CHECKER",
                               "Dangerous Properties with key : $key and bad value : $badValue"
                            )
                            result = true
                        }
                    }
                }
            }
            return result
        }

    //canChangePermission
    private val isHaveReadWritePermission: Boolean
        private get() {
            var result = false
            val lines = commander("mount")
            for (line in lines!!) {
                val args = line.split(" ".toRegex()).toTypedArray()
                if (args.size < 4) {
                    continue
                }
                val mountPoint = args[1]
                val mountOptions = args[3]
                for (path in notWritablePath) {
                    if (mountPoint.equals(path, ignoreCase = true)) {
                        for (opt in mountOptions.split(",".toRegex()).toTypedArray()) {
                            if (opt.equals("rw", ignoreCase = true)) {
                                Log.e(
                                    "ROOT_CHECKER",
                                    "Path : $path is mounted with read write permission$line"
                                )
                                result = true
                                break
                            }
                        }
                    }
                }
            }
            return result
        }

    private fun commander(command: String): Array<String>? {
        return try {
            val inputStream = Runtime.getRuntime().exec(command).inputStream ?: return null
            val propVal = Scanner(inputStream).useDelimiter("\\A").next()
            propVal.split("\n".toRegex()).toTypedArray()
        } catch (e: Exception) {
            e.printStackTrace()
            null
        }
    }

    private fun isAnyPackageFromListInstalled(pkg: ArrayList<String>): Boolean {
        var result = false
        val pm: PackageManager =
            RootCheckerPlugin.Companion.vRegistrar.activeContext().getPackageManager()
        for (packageName in pkg) {
            try {
                pm.getPackageInfo(packageName, 0)
                result = true
            } catch (e: Exception) {
            }
        }
        return result
    }


    companion object {
        /** Plugin registration.  */
        lateinit var vRegistrar: Registrar
        fun registerWith(registrar: Registrar) {
            RootCheckerPlugin.Companion.vRegistrar = registrar
            val channel = MethodChannel(registrar.messenger(), "root_checker")
            channel.setMethodCallHandler(RootCheckerPlugin())
        }
    }
}
```

## C++

```c++


void RootCheckerPlugin::registerWith(Registrar *registrar)
{
  vRegistrar = registrar;
  MethodChannel * const channel = new MethodChannel(registrar->messenger(), "root_checker");
  RootCheckerPlugin tempVar();
  channel->setMethodCallHandler(&tempVar);
}

void RootCheckerPlugin::onMethodCall(MethodCall *call, MethodChannel::Result *result)
{
  if (call->method.equals("isDeviceRooted"))
  {
	checkHpBodong(result);
  }
  else
  {
	result->notImplemented();
  }
}

void RootCheckerPlugin::checkHpBodong(MethodChannel::Result *result)
{
  result->success(isPathExist("su") || isSUExist() || isTestBuildKey() || isHaveDangerousApps() || isHaveRootManagementApps() || isHaveDangerousProperties() || isHaveReadWritePermission());
}


bool RootCheckerPlugin::isSUExist()
{
  Process *process = nullptr;
  try
  {
	process = Runtime::getRuntime().exec(std::vector<std::string>{"/system/xbin/which", "su"});
	InputStreamReader tempVar(process->getInputStream());
	BufferedReader *in = new BufferedReader(&tempVar);
	if (in->readLine() != nullptr)
	{
	  Log::e("ROOT_CHECKER","cammand executed");

	  delete in;
	  return true;
	}

	delete in;
	return false;
  }
  catch (const std::runtime_error &e)
  {
	return false;
  }

  {
	if (process != nullptr)
	{
	  process->destroy();
	}
  }
}

bool RootCheckerPlugin::isTestBuildKey()
{
  std::string buildTags = Build::TAGS;
  if (buildTags != "" && buildTags.find("test-keys") != std::string::npos)
  {
	Log::e("ROOT_CHECKER","devices buid with test key");
	return true;
  }
  return false;
}

bool RootCheckerPlugin::isHaveDangerousApps()
{
  std::vector<std::string> packages;
  packages.addAll(Arrays::asList(ConstantCollections::dangerousListApps));
  return isAnyPackageFromListInstalled(packages);
}

bool RootCheckerPlugin::isHaveRootManagementApps()
{
  std::vector<std::string> packages;
  packages.addAll(Arrays::asList(ConstantCollections::rootsAppPackage));
  return isAnyPackageFromListInstalled(packages);
}

bool RootCheckerPlugin::isHaveDangerousProperties()
{

  std::unordered_map<std::string, std::string> dangerousProps;
  dangerousProps.emplace("ro.debuggable", "1");
  dangerousProps.emplace("ro.secure","0");

  bool result = false;
  std::vector<std::string> lines = commander("getprop");
  if (lines.empty())
  {
	return false;
  }
  for (auto line : lines)
  {
	for (auto key : dangerousProps)
	{
	  if (line.contains(key.first))
	  {
		std::string badValue = dangerousProps[key.first];
		badValue = "[" + badValue + "]";
		if (line.contains(badValue))
		{
		  Log::e("ROOT_CHECKER","Dangerous Properties with key : " + key.first + " and bad value : " + badValue);
		  result = true;
		}
	  }
	}
  }
  return result;
}


```