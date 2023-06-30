`timescale 1ns / 1ps


module TB_main();

logic [2:0] number;
logic [7:0] code;


main DUT(.number(number), .code(code));

initial begin
    number = 3'b000; #1;

    repeat (7) begin
        number = number + 1; #1;
    end

$finish;
end

endmodule       