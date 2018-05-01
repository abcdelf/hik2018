#include <iostream>
#include <string>
#include "json/json.h"   

using namespace std;          
int main(int argc, char **argv)   {

  if (argc != 4) {
      cout << "Wrong arguments." << endl;
      return -1;
  }

  printf("Server Host: %s\nServer Port: %s\nAuth Token: %s\n", argv[1], argv[2], argv[3]);
  cout << "=================================\nJSON Example:" << endl;

  Json::Value root;
  Json::FastWriter fast;
        
  root["x"] = Json::Value(100);
        
  root["y"] = Json::Value(200);
        
  root["z"] = Json::Value(300);
        
  root["comment"] = Json::Value("Hello, AI!");
        
  cout<<fast.write(root)<<endl;
        
  return 0;
}