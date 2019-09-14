#ifndef SmoothTransition_class_h
#define SmoothTransition_class_h

class SmoothTransition {
    public:
        SmoothTransition(int speed = 200, int defaultValue = 0);
        ~SmoothTransition();

        void setTargetValue(int targetValue);
        
        int getCurrentValue();
        bool isFinished();

    private:
        int currentValue;
        int speed;
        int startValue;
        int targetValue;
        unsigned long transitionStarted;
};

#endif
