program CALC_PROG {
    version CALC_VERS {
        char CONVERT_OPERATOR(char*) = 1;
        double DO_CALCULATION(para) = 2;
        double CALCULATE_EXPRESSION(char*) = 3;
    } = 1;
} = 0x21111111;
