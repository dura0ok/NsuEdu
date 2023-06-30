`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 18.02.2022 20:41:36
// Design Name: 
// Module Name: main
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module main(
    input a,
    input b,
    input carry,
    output our,
    output next
    );
    assign our = (!a & !b & carry) | (!a & b & !carry) | (a & !b & !carry) | (a & b & carry);
    assign next = (!a & !b & carry) | (!a & b & !carry) | (!a & b & carry) | (a & b & carry);
endmodule
