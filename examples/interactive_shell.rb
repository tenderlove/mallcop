# Load this file with IRB:
#
#   $ irb -I lib examples/interactive_shell.rb

require 'mallcop'

# "72.233.42.42", "carllerche"

public_key  = File.expand_path("~/.ssh/id_dsa.pub")
private_key = File.expand_path("~/.ssh/id_dsa")

@c = Socket.new(Socket::AF_INET, Socket::SOCK_STREAM, 0)
puts "=== Connecting to localhost"
@c.connect(Socket.sockaddr_in(22, '127.0.0.1'))

session = MallCop::Session.new @c
puts "=== Starting SSH session"
session.start
puts "=== Authenticating from public key"
unless session.userauth_publickey_fromfile(ENV['USER'], public_key, private_key, '')
  raise "Authentication failed"
end

puts "=== Opening channel"
chan = session.open_channel

if ENV['PTY']
  puts "=== Requesting PTY"
  unless chan.request_pty("vanilla")
    raise "PTY request failed"
  end

  puts "=== Starting shell"
  unless chan.shell
    raise "Shell request failed"
  end
else
  puts "=== Executing BASH"
  chan.channel_exec("bash -l")
end

C = chan

# C.write("pwd\n")
# C.read
