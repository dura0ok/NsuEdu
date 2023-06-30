`timescale 1ns / 1ps


module TB_main();

logic [7:0] number;
logic [2:0] code;

main DUT(.number(number), .code(code));

initial begin
    number = 8'b00000000; #1;

    repeat (255) begin
        number = number + 1; #1;
    end

$finish;
end

endmodule
