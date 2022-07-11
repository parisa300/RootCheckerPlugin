package com.example.pluginforc

import android.content.pm.PackageManager
import java.io.BufferedReader
import java.io.File
import java.io.InputStreamReader
import java.lang.Exception
import java.util.*
import kotlin.collections.HashMap
import io.flutter.plugin.common.MethodCall

import io.flutter.plugin.common.MethodChannel

import io.flutter.plugin.common.MethodChannel.MethodCallHandler

import io.flutter.plugin.common.MethodChannel.Result

import io.flutter.plugin.common.PluginRegistry.Registrar

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
                // Log.e("ROOT_CHECKER", "Path is exist : $joinPath")
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
                    //    Log.e("ROOT_CHECKER", "cammand executed")
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
                //     Log.e("ROOT_CHECKER", "devices buid with test key")
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
//                            Log.e(
//                                "ROOT_CHECKER",
//                                "Dangerous Properties with key : $key and bad value : $badValue"
//                            )
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
//                                Log.e(
//                                    "ROOT_CHECKER",
//                                    "Path : $path is mounted with read write permission$line"
//                                )
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



