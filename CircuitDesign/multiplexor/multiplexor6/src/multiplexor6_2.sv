`timescale 1ns / 1ps

module multiplexor3(
    input [7:0] in,
    input [2:0] out,
    output out
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
    
    genvar i;
        generate
        for (i = 0; i < 8; i++)
        begin: label
            multiplexor3 multiplexor(.in(in[i*8+7:i*8]), .out(out[2:0]), .out(temp[i])); 
        end
        endgenerate       

    
    multiplexor3 m8(.in({temp[7], temp[6], temp[5], temp[4], temp[3], temp[2], temp[1], temp[0]}), .out(out[5:3]), .out(out));
endmodule
