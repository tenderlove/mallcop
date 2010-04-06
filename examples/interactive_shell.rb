# Load this file with IRB

$:.unshift File.expand_path('../../lib', __FILE__)
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
unless session.userauth_publickey_fromfile('carllerche', public_key, private_key, '')
  raise "Authentication failed"
end

puts "=== Opening channel"
chan = session.open_channel

puts "=== Requesting PTY"
unless chan.request_pty("xterm")
  raise "PTY request failed"
end

puts "=== Starting shell"
unless chan.shell
  raise "Shell request failed"
end

C = chan

# C.read # => read the prompt
# C.write("pwd\n")
# C.read