`timescale 1ns / 1ps


module TB_main();
logic [31:0] a, b;
logic [7:0]result;
main DUT(.a(a), .b(b), .result(result));
initial begin
     //a = {8'b01111111, 24'b0}; b = {8'b00000000, 24'b0};  #10; 
     a =120; b = 95;  #10; 
 
$finish;
end
endmodule
