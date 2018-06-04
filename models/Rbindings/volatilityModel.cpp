#include "volatilityModel.h"
#include <iostream>

volatilityModel::volatilityModel(): graphPath(""), R(new RInside()) {

}

/* The library and package path need to be configured on deployed server */
/* Only then, this function works as expected */
void volatilityModel::checkAndInstallPackages() {
  std::cout << "checking installed packages" << std::endl;

  std::string cmd = "packageList <- installed.packages()";
  Rcpp::DataFrame df = R->parseEval(cmd);

  if(df.containsElementNamed("Package")) {
    // trace out the installed packages
    Rcpp::CharacterVector cv = df["Package"];
    for(Rcpp::CharacterVector::iterator it = cv.begin(); it != cv.end(); it++) {
      std::cout << (*it)  << std::endl;
    }

    // check for model specific packages
    bool factoMineR = false;
    bool factoExtra = false;
    for (Rcpp::CharacterVector::iterator it = cv.begin(); it != cv.end(); it++) {
      std::cout << "installed package "  << (*it) << std::endl;
      if (*it == Rcpp::String("FactoMineR")) {
        std::cout << "FactoMineR is already installed" << std::endl;
        factoMineR = true;
      }
      if (*it == Rcpp::String("factoextra")) {
        std::cout << "FactoMineR is already installed" << std::endl;
        factoExtra = true;
      }
    }

    if (!factoMineR) {
      std::cout << "installing factoMineR" << std::endl;
      // choose the right and fastest mirror (redirecting to SNU, Seoul currently)
      std::string cmd = "install.packages(\"FactoMineR\", repos = \"http://healthstat.snu.ac.kr/CRAN/\")";
      R->parseEval(cmd);
    }

    if (!factoExtra) {
      std::cout << "installing factoExtra" << std::endl;
      // choose the right and fastest mirror (redirecting to SNU, Seoul currently)
      std::string cmd = "install.packages(\"factoextra\", repos = \"http://healthstat.snu.ac.kr/CRAN/\")";
      R->parseEval(cmd);
    }
  }
}

void volatilityModel::configure(std::string graphLocation, std::string dataSet,  bool enableVisualPlots) {
  std::cout << "configure " << graphLocation << std::endl;

  // update paths
  this->graphPath = graphLocation;
  this->data = dataSet;

  checkAndInstallPackages();

  // load FactoMineR and factoextra packages
  std::string cmd = "library(\"FactoMineR\");library(\"factoextra\");library(\"ggplot2\")";
  R->parseEval(cmd);

  if (enableVisualPlots) {
    std::cout << "enabling graph capture" << std::endl;
    enableGraphCapture();
  }
}

void volatilityModel::executePCA() {
  std::cout << "executePCA" << std::endl;

  // First read in the dataset
  std::string readCmd = "dataset <- read.csv(\""  + this->data + "\");";
  R->parseEvalQ(readCmd);

  // execute the PCA now
  std::cout << "executing PCA now"  << std::endl;
  // Force capture Factor Plots
  enableGraphCapture();

  std::string pcaCmd = "res <- PCA(log(dataset))";
  Rcpp::List pcaDf = R->parseEval(pcaCmd);

  // turn capture device off
  std::string devoff = "dev.off();";
  R->parseEval(devoff);

  std::cout << "done with PCA"  << std::endl;

  // trace the PC's and corresponding eigenValues
  if (pcaDf.containsElementNamed("eig")) {
    std::cout << "acessing eigenValues" << std::endl;
    Rcpp::DataFrame principalComponents = pcaDf["eig"];

    // populate all components in result set
    int rows = principalComponents.nrows();
    int columns = principalComponents.length();
    Rcpp::CharacterVector pc = principalComponents.names();

    std::stringstream dataframesize;
    dataframesize << "dataframe with size " << rows << " " << columns;
    std::cout << dataframesize.str() << std::endl;

    // cannot index a dataframe by row and column
    Rcpp::NumericMatrix values = Rcpp::internal::convert_using_rfunction(principalComponents, "as.matrix");

    for (int row = 0; row < rows; row++) {
      std::stringstream stream;
      stream << "component " << row;
      std::string componentName = stream.str();
      std::map<std::string, double> component;
      for(int column = 0; column < columns; column++) {
        std::stringstream stream;
        stream << "value at row " << row << " column " << column;
        std::cout << stream.str()  << std::endl;
        std::cout << values(row, column)  << std::endl;
        char* str = pc[column];
        std::cout << str << std::endl;
        std::string name(str);
        component[name] = values(row, column);
      }
      principalComponents[componentName] = component;
    }
  }

  // trace the loadings of variables
  if (pcaDf.containsElementNamed("var")) {
    std::cout << "acessing variables" << std::endl;
    Rcpp::List variables = pcaDf["var"];
    Rcpp::DataFrame loadings = variables["contrib"];

    // populate all components in result set
    int rows = loadings.nrows();
    int columns = loadings.length();

    std::string rowNameCmd = "cv  <- c(rownames(res$var$contrib))";
    Rcpp::CharacterVector vVariables = R->parseEval(rowNameCmd);

    // trace variable names
    for (auto iter = vVariables.begin(); iter != vVariables.end(); iter++) {
      std::cout << *iter  << std::endl;
    }

    // cannot index a dataframe by row and column, hence, transitioning to a matrix
    Rcpp::NumericMatrix matrix = Rcpp::internal::convert_using_rfunction(loadings, "as.matrix");
    Rcpp::CharacterVector rownames = Rcpp::rownames(matrix);
    Rcpp::CharacterVector columnnames = Rcpp::colnames(matrix);

    int rcounter = 0;
    int ccounter = 0;

    for (int rcounter = 0; rcounter < rows; rcounter++) {
      std::string variable(vVariables[rcounter]);
      std::cout << "variable name " << variable << std::endl;
      std::map<std::string, double> loadingMap;
      for (int ccounter = 0; ccounter < columns; ccounter++) {
        std::string component(columnnames[ccounter]);
        std::cout << "contribution to component " << component << std::endl;
        std::cout << matrix(rcounter, ccounter) << std::endl;
        loadingMap[component] = matrix(rcounter, ccounter);
      }
      variableLoadings[variable] = loadingMap;
    }
  }
}

int volatilityModel::run() {

  std::cout << "Volatility Model run" << std::endl;
  checkAndInstallPackages();
  executePCA();
  return 0;
}

std::string volatilityModel::getVersion() {
  return "version1";
}

std::map<std::string, std::map<std::string, double> > volatilityModel::getPrincipalComponents() {
  return principalComponents;
}

std::map<std::string, std::map<std::string, double> > volatilityModel::getVariableLoadings() {
  return variableLoadings;
}

void volatilityModel::enableGraphCapture() {
  std::cout << "enabling graph capture" << std::endl;
  std::cout << "graphPath is " + this->graphPath << std::endl;
  std::string cmd = "png('" + this->graphPath + "')";
  std::cout << cmd << std::endl;
  R->parseEval(cmd);
}
