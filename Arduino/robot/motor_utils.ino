#include "headers/motor_utils.h"

BLA::Matrix<2,3,float> vmatrix_pinv={
     0.47140452,  0.17254603, -0.64395055,
     -0.47140452,  0.64395055, -0.17254603
};
BLA::Matrix<1, 3, float> getCoefficents(){

    ControllerPtr myController = myControllers[0];

    if (myController && myController->isConnected() && myController->hasData()) {
            if (myController->isGamepad()) {
                
                return coefficentCalculator();
            } else {
                Serial.println("Unsupported controller");
            }
    }
    return BLA::Zeros<1, 3, float>();
}

BLA::Matrix<1, 3, float> coefficentCalculator(){

    ControllerPtr myController = myControllers[0];
    BLA::Matrix<1, 3, float> coeff;
    coeff.Fill(0.0);
    if(myController->brake() > 20){
        float coeff_val = myController->brake() / -1024.0;
        coeff.Fill(coeff_val);
        return coeff;
    }
    if(myController->throttle() > 20){
        float coeff_val = myController->throttle() / 1024.0;
        coeff.Fill(coeff_val);
        return coeff;
    }
    coeff = getLeftStickVector(myControllers[0]) * vmatrix_pinv;
    return coeff;
}