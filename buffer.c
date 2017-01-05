// 8kHz Bandstop Buttterworth Filter Coefficients
static float bandstop_g = 0.9873701369842;
static float bandstop_num[3] = {0.9873701369842, -1.058453023, 0.9873701369842};
static float bandstop_den[3] = {1, -1.058453022724, 0.9747402739684};

float bandstop_buf[3] = {0,0,0};

void update_buffer(float *w){
    
    w[0] = w[1];              //First buffer value replaced by second value
    w[1] = w[2];              //Second buffer value replaced by third value
    
}

float filter(float x, float *w, float *b, float *a){
    
    w[2] = x - (a[1] * w[1]) - (a[2] * w[0]);                     //Last value of bandstop_buf is changed to (input) - (2nd denominator * 2nd of bandstop_buf value) - (3rd denominator * 1st bandstop_buf value)
    float y = (b[0] * w[2]) + (b[1] * w[1]) + (b[2] * w[0]);      //Output is equal to (first numerator * last bandstop_buf) + (second numerator * second bandstop_buf) + (third numerator * first bandstop_buf)
    
    return y;
    
}

float bandstop(float input){
    
    update_buffer(bandstop_buf); //Update the first and second values of the buffer
    float output = filter(input, bandstop_buf, bandstop_num, bandstop_den);   //Update the third value of the buffer and acquire final output
    
    return output * bandstop_g;  //Return output multiplied by buffer gain
