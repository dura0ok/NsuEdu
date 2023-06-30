`timescale 1ns / 1ps

module main(
    input [7:0] code,
    output [2:0] number
    );
    assign number[0] = code[1] | code[3] | code[5] | code[7];
    assign number[1] = code[2] | code[3] | code[6] | code[7];
    assign number[2] = code[4] | code[5] | code[6] | code[7];
endmodule
