`timescale 1ns / 1ps


module main(
    input [7:0] number,
    output logic [2:0] code
    );
    
    always_comb
        if (number[7])      code = 3'b111;
        else if (number[6]) code = 3'b110;
        else if (number[5]) code = 3'b101;
        else if (number[4]) code = 3'b100;
        else if (number[3]) code = 3'b011;
        else if (number[2]) code = 3'b010;
        else if (number[1]) code = 3'b001;
        else            code = 3'b000;
endmodule
