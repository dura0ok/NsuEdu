`timescale 1ns / 1ps

module main(
    input [2:0] number,
    output [7:0] code
    );
    
    
    assign code[0]=!number[0] & !number[1] & !number[2];
    assign code[1]= number[0] & !number[1] & !number[2];
    assign code[2]=!number[0] &  number[1] & !number[2];
    assign code[3]= number[0] &  number[1] & !number[2];
    assign code[4]=!number[0] & !number[1] &  number[2];
    assign code[5]= number[0] & !number[1] &  number[2];
    assign code[6]=!number[0] &  number[1] &  number[2];
    assign code[7]= number[0] &  number[1] &  number[2];
    
endmodule
