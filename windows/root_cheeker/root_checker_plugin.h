



using ArrayList = java::util::ArrayList;

using MethodCall = io::flutter::plugin::common::MethodCall;
using MethodChannel = io::flutter::plugin::common::MethodChannel;
using MethodCallHandler = io::flutter::plugin::common::MethodChannel::MethodCallHandler;
using Result = io::flutter::plugin::common::MethodChannel::Result;
using Registrar = io::flutter::plugin::common::PluginRegistry::Registrar;

/** RootCheckerPlugin */
class RootCheckerPlugin : public MethodChannel::MethodCallHandler
{
  /** Plugin registration. */
  public:
  static Registrar *vRegistrar;

  static void registerWith(Registrar *registrar);

  void onMethodCall(MethodCall *call, MethodChannel::Result *result) override;

  private:
  void checkHpBoding(MethodChannel::Result *result);

  bool isPathExist(const std::string &ext);

  bool isSUExist();

  bool isTestBuildKey();

  bool isHaveDangerousApps();

  bool isHaveRootManagementApps();


  //check dangerous properties
  bool isHaveDangerousProperties();

  //canChangePermission
  bool isHaveReadWritePermission();

  std::vector<std::string> commander(const std::string &command);

  bool isAnyPackageFromListInstalled(std::vector<std::string> &pkg);
};

