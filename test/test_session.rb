require "test/unit"
require "mallcop"
require 'psych'

class TestSession < Test::Unit::TestCase
  def setup
    @c = Socket.new(Socket::AF_INET, Socket::SOCK_STREAM, 0)
    @c.connect(Socket.sockaddr_in(22, '127.0.0.1'))
    @config = Psych.load_file File.join ENV['HOME'], '.mallcop_test'
  end

  def test_session
    session = MallCop::Session.new @c
  end

  def test_start
    session = MallCop::Session.new @c
    session.start
  end

  def test_hostkey_hash
    session = MallCop::Session.new @c
    session.start
    hash = session.hostkey_hash MallCop::Session::HASH_SHA1
    assert_instance_of String, hash
  end

  def test_userauth_list
    session = MallCop::Session.new @c
    session.start
    list = session.authlist_for @config['username']
    assert_instance_of Array, list
    assert list.include?('publickey')
  end
end
