`timescale 1ns / 1ps


module TB_main();

logic [7:0] code;
logic [2:0] number;


main DUT(.code(code), .number(number));

initial begin
    code = 8'b00000000; #1;
    code = 8'b00000001; #1;

    repeat (7) begin
        code = code * 2; #1;
    end

$finish;
end

endmodule