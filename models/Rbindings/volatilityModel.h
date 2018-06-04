#ifndef __VOLATILITY_MODEL__
#define __VOLATILITY_MODEL__

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <sstream>
#include <stdint.h>
#include "RInside.h"

// TBD: We could refactor certain functionality in base class(ex: model),
//      which every model could derive from
//      Not doing that now, since priority is to establish first model quickly.

class volatilityModel {
  // variables
  private:
  std::unique_ptr<RInside> R;
  std::string version;
  std::string graphPath;
  std::string data;
  std::map<std::string, std::map<std::string, double> > principalComponents;
  std::map<std::string, std::map<std::string, double> > variableLoadings;

  // methods
  private:
  void enableGraphCapture();
  void executePCA();
  void checkAndInstallPackages();

  // methods
  public:

  volatilityModel();
  std::string getVersion();
  void configure(std::string graphLocation, std::string dataSet,  bool enableVisualPlots);
  std::map<std::string, std::map<std::string, double> > getPrincipalComponents();
  std::map<std::string, std::map<std::string, double> > getVariableLoadings();
  int run();
};

#endif
