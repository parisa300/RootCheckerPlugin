

#include "ConstantCollection.h"


{

const std::vector<std::string> ConstantCollections::rootsAppPackage = {
  "com.noshufou.android.su",
  "com.noshufou.android.su.elite",
  "eu.chainfire.supersu",
  "com.koushikdutta.superuser",
  "com.thirdparty.superuser",
  "com.yellowes.su",
  "com.topjohnwu.magisk"};
const std::vector<std::string> ConstantCollections::dangerousListApps = {
  "com.koushikdutta.rommanager",
  "com.koushikdutta.rommanager.license",
  "com.dimonvideo.luckypatcher",
  "com.chelpus.lackypatch",
  "com.ramdroid.appquarantine",
  "com.ramdroid.appquarantinepro",
  "com.android.vending.billing.InAppBillingService.COIN",
  "com.chelpus.luckypatcher"};
const std::vector<std::string> ConstantCollections::rootCloakingApps = {
  "com.devadvance.rootcloak",
  "com.devadvance.rootcloakplus",
  "de.robv.android.xposed.installer",
  "com.saurik.substrate",
  "com.zachspong.temprootremovejb",
  "com.amphoras.hidemyroot",
  "com.amphoras.hidemyrootadfree",
  "com.formyhm.hiderootPremium",
  "com.formyhm.hideroot"};
const std::vector<std::string> ConstantCollections::superUserPath = {
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
  "/dev"};
const std::vector<std::string> ConstantCollections::notWritablePath = {
 "/system",
 "/system/bin",
 "/system/sbin",
 "/system/xbin",
 "/vendor/bin",
 "/sbin",
 "/etc"};
}
