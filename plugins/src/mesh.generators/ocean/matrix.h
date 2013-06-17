/**
* Project: VSXu: Realtime visual programming language, music/audio visualizer, animation tool and much much more.
*
* @author Joe Dart. Cleaned up for VSXu by Jonatan "jaw" Wallmander
*
* License: Public Domain
*/

//
// Helper functions for matrix and vector math.
//

#define TO_RADS 0.017453293f

void setIdentity(float a[16]);
void multMatrices(float a[16], float b[16]);  //this may not back multiply
void mytranslate(float a[16],float x,float y,float z);
void myrotate(float a[16],float x,float y,float z);
void normalize(double vec[]);
void cross_prod(double in1[],double in2[],double out[3]);

