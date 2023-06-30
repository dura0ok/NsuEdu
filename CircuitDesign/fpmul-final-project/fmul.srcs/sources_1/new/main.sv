`timescale 1ns / 1ps


module main(
    input logic [31:0] a,
    input  logic [31:0] b,
    output [31:0] result
    );
    
    // first step
   
   
   logic [7:0] exp1;  assign exp1 = a[8:1];
   logic [7:0] exp2; assign exp2 = b[8:1];
   logic [7:0] new_exp;  assign new_exp = exp1 + exp2;
   
   
   // second step
   logic [24:0] first_mantissa; assign first_mantissa = a[31:9];
   logic [24:0] second_mantissa; assign second_mantissa = b[31:9];
   logic [24:0] new_mantissa; assign new_mantissa = a * b;

   // third step
   logic [1:0] sign;
   assign sign = a[1:0] ^ b[1:0];
   
   assign result = {sign, new_exp, new_mantissa};

   
    
endmodule
