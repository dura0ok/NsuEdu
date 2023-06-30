`timescale 1ns / 1ps

module main(
    input [7:0] in,
    input [2:0] sel,
    output logic out
    );
    
//    always_comb
//        if (sel == 3'b000) out = in[0];
//        else if(sel == 3'b001) out = in[1];
//        else if(sel == 3'b010) out = in[2];
//        else if(sel == 3'b011) out = in[3];
//        else if(sel == 3'b100) out = in[4];
//        else if(sel == 3'b101) out = in[5];
//        else if(sel == 3'b110) out = in[];
//        else if(sel == 3'b111) out = in[0];
           
//         case(sel)
//            3'b000 : out = in[0];          
//            3'b001 : out = in[1];          
//            3'b010 : out = in[2];          
//            3'b011 : out = in[3];          
//            3'b100 : out = in[4];          
//            3'b101 : out = in[5];          
//            3'b110 : out = in[6];          
//            3'b111 : out = in[7];          
            
//        endcase; 


//assign out = (!sel[0] & !sel[1] & !sel[2] & in[0]) |
//                       (sel[0] & !sel[1] & !sel[2] & in[1]) |
//                       (!sel[0] & sel[1] & !sel[2] & in[2]) |
//                       (sel[0] & sel[1] & !sel[2] & in[3]) |
//                       (!sel[0] & !sel[1] & sel[2] & in[4]) |
//                       (sel[0] & !sel[1] & sel[2] & in[5]) |
//                       (!sel[0] & sel[1] & sel[2] & in[6]) |
//                       (sel[0] & sel[1] & sel[2] & in[7]);

endmodule
