//
// Created by Pixel on 7/3/2022.
//

#include "rootCheckerPlugin.h"
#include "ConstantCollection.cpp"
using PackageManager = android::content::pm::PackageManager;
using Build = android::os::Build;
using Log = android::util::Log;
using BufferedReader = java::io::BufferedReader;
using File = java::io::File;
using InputStream = java::io::InputStream;
using InputStreamReader = java::io::InputStreamReader;
using ArrayList = java::util::ArrayList;
using Arrays = java::util::Arrays;
using HashMap = java::util::HashMap;
using Map = java::util::Map;
using Scanner = java::util::Scanner;
using MethodCall = io::flutter::plugin::common::MethodCall;
using MethodChannel = io::flutter::plugin::common::MethodChannel;
using MethodCallHandler = io::flutter::plugin::common::MethodChannel::MethodCallHandler;
using Result = io::flutter::plugin::common::MethodChannel::Result;
using Registrar = io::flutter::plugin::common::PluginRegistry::Registrar;
io::flutter::plugin::common::PluginRegistry::Registrar *RootCheckerPlugin::vRegistrar;

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

bool RootCheckerPlugin::isPathExist(const std::string &ext)
{
  for (std::string path : ConstantCollections::superUserPath)
  {
	std::string joinPath = path + ext;
	File *file = new File(path, ext);
	if (file->exists())
	{
	  Log::e("ROOT_CHECKER","Path is exist : " + joinPath);

	  delete file;
	  return true;
	}

	  delete file;
  }
  return false;
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

bool <missing_class_definition>::isHaveReadWritePermission()
{
  std::optional<bool> result = false;
  std::vector<std::string> lines = commander("mount");

  for (auto line : lines)
  {
	std::vector<std::string> args = line.split(" ");
	if (args.size() < 4)
	{
	  continue;
	}
	std::string mountPoint = args[1];
	std::string mountOptions = args[3];

	for (std::string path : ConstantCollections::notWritablePath)
	{

	  if (mountPoint.equalsIgnoreCase(path))
	  {
		for (std::string opt : mountOptions.split(","))
		{
		  if (opt.equalsIgnoreCase("rw"))
		  {
			Log::e("ROOT_CHECKER","Path : " + path + " is mounted with read write permission" + line);
			result = true;
			break;
		  }
		}
	  }
	}
  }

  return result;
}

std::vector<std::string> <missing_class_definition>::commander(const std::string &command)
{
  try
  {
	InputStream *inputStream = Runtime::getRuntime().exec(command).getInputStream();
	if (inputStream == nullptr)
	{
	  return std::vector<std::string>();
	}
	Scanner tempVar(inputStream);
	std::string propVal = (&tempVar)->useDelimiter("\\A")->next();
	return propVal.split("\n");
  }
  catch (const std::runtime_error &e)
  {
	e.printStackTrace();
	return std::vector<std::string>();
  }
}

bool <missing_class_definition>::isAnyPackageFromListInstalled(std::vector<std::string> &pkg)
{
  bool result = false;
  PackageManager *pm = vRegistrar::activeContext().getPackageManager();
  for (auto packageName : pkg)
  {
	try
	{
	  pm->getPackageInfo(packageName,0);
	  result = true;
	}
	catch (const std::runtime_error &e)
	{

	}
  }
  return result;
}
