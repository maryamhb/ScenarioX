// Bandstop Filter Implementation
// 8kHz Bandstop Butterworth Filter Coefficients from MATLAB
static float gain = 0.9875889380;
static float num[3] = { 1, -1.07173820, 1}; //numerators
static float den[3] = { 1, -1.05843680, 0.975177876}; //denominators
float buffer[3] = {0,0,0}; //buffer implementation

void update_buffer(float *w){
    w[0] = w[1];	//First buffer value replaced by the second
    w[1] = w[2];	//Second buffer value replaced by the third
}

//filter function, where a->numerators, b->denominators, w->past outputs (buffer)
float filter(float x, float *w, float *b, float *a){
    w[2] = x - (a[1] * w[1]) - (a[2] * w[0]); //last buffer value
    float y = (b[0] * w[2]) + (b[1] * w[1]) + (b[2] * w[0]); 
    return y;
}

float bandstop(float input){
    update_buffer(buffer); //Update the buffer values (first and second)
    float output = filter(input, buffer, num, den); //Update the third value of the buffer and acquire final output
    
    return output * gain;	//Return output multiplied by buffer gain
}
