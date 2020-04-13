class Mutex{
private:
    int value;
    register reg;
    // required member variable(s)
public:
    Mutex (){
        // define the constructor for initialization
        int temp = 0;
        sawp(&temp,  reg);
    }
    Lock (){
        // can only use temporary variables, any registers and ... 
        // the atomic swap (var, $reg) instruction
        // you cannot directly access a register w/o using swap
        do {
            int temp = 1;
            swap(&temp, reg);
            value = temp;
        }while(value == 1)
    }
    Unlock (){
        int temp = 0;
        swap(&temp, reg);
        // same restriction as in the Lock function
    }
};