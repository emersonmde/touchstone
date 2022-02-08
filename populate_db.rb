#!/usr/bin/env ruby


def run_script(commands)
  raw_output = nil
  IO.popen("./cmake-build-debug/touchstone ./cmake-build-debug/test.db", "r+") do |pipe|
    commands.each do |command|
      pipe.puts command
    end

    pipe.close_write

    # Read entire output
    raw_output = pipe.gets(nil)
  end
  raw_output.split("\n")
end

script = (1..30).to_a.shuffle.map do |i|
  "insert #{i} user#{i} user#{i}@example.com"
end
script << ".print_tree"
script << ".exit"
result = run_script(script)

puts(result)
