#line 1 "G:\\MyProjects\\TriangularRobot\\Arduino\\robot\\headers\\controller_utils.h"
#include <Bluepad32.h>
#include <BasicLinearAlgebra.h>
#include <ElementStorage.h>


extern ControllerPtr myControllers[BP32_MAX_GAMEPADS];

void onConnectedController(ControllerPtr ctl);
void onDisconnectedController(ControllerPtr ctl);
void dumpGamepad(ControllerPtr ctl) ;
void processGamepad(ControllerPtr ctl);
void processControllers() ;
void controller_startup();
BLA::Matrix<1, 2, float> getLeftStickVector(ControllerPtr ctl);