`timescale 1ns / 1ps


module main(
    input [7:0] number,
    output [2:0] code
    );
    
    assign code[2] = number[7] | number[6] | number[5] | number[4];
    assign code[1] = number[7] | number[6] | (!number[5] & !number[4] & (number[3] | number[2]));
    assign code[0] = number[7] | (!number[6] & number[5]) | (!number[6] & !number[5] & !number[4] & number[3]) | (!number[6] & !number[5] & !number[4] & !number[3] & !number[2] & number[1]);
    
endmodule
