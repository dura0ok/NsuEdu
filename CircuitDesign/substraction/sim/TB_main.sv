`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 18.02.2022 20:42:25
// Design Name: 
// Module Name: TB_main
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


module TB_main();

logic a, b, carry, our, next;

main DUT(.a(a), .b(b), .carry(carry), .our(our), .next(next));
initial begin
    a = 1'b0; b = 1'b0; carry = 1'b0; #10; 
    a = 1'b0; b = 1'b0; carry = 1'b1; #10;
    a = 1'b0; b = 1'b1; carry = 1'b0; #10;
    a = 1'b0; b = 1'b1; carry = 1'b1; #10;
    a = 1'b1; b = 1'b0; carry = 1'b0; #10;
    a = 1'b1; b = 1'b0; carry = 1'b1; #10;
    a = 1'b1; b = 1'b1; carry = 1'b0; #10;
    a = 1'b1; b = 1'b1; carry = 1'b1; #10;
$finish;
end
endmodule
