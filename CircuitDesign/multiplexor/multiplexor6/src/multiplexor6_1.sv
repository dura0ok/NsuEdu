`timescale 1ns / 1ps

module multiplexor3(
    input [7:0] in,
    input [2:0] out,
    output out,
    );
        
     assign out = (!out[0] & !out[1] & !out[2] & in[0]) |
                  (out[0] & !out[1] &  !out[2] & in[1]) |
                  (!out[0] &  out[1] & !out[2] & in[2]) |
                  (out[0] &  out[1] &  !out[2] & in[3]) |
                  (!out[0] & !out[1] & out[2] & in[4]) |
                  (out[0] & !out[1] &  out[2] & in[5]) |
                  (!out[0] &  out[1] & out[2] & in[6]) |
                  (out[0] &  out[1] &  out[2] & in[7]);        
endmodule


module multiplexor6(
    input [63:0] in,
    input [5:0] out,
    output out
    );    
    
    logic temp[7:0];
    
    multiplexor3 m0(.in(in[7:0]),   .out(out[2:0]), .out(temp[0]));
    multiplexor3 m1(.in(in[15:8]),  .out(out[2:0]), .out(temp[1]));
    multiplexor3 m2(.in(in[23:16]), .out(out[2:0]), .out(temp[2]));
    multiplexor3 m3(.in(in[31:24]), .out(out[2:0]), .out(temp[3]));
    multiplexor3 m4(.in(in[39:32]), .out(out[2:0]), .out(temp[4]));
    multiplexor3 m5(.in(in[47:40]), .out(out[2:0]), .out(temp[5]));
    multiplexor3 m6(.in(in[55:48]), .out(out[2:0]), .out(temp[6]));
    multiplexor3 m7(.in(in[63:56]), .out(out[2:0]), .out(temp[7]));
    
    multiplexor3 m8(.in({temp[7], temp[6], temp[5], temp[4], temp[3], temp[2], temp[1], temp[0]}), .out(out[5:3]), .out(out));
endmodule
