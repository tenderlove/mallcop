require "helper"

describe "interactive" do

  def config
    @config ||= YAML.load_file "#{root}/.mallcop_user"
  end

  def username
    config['username']
  end

  def password
    config['password']
  end

  before do
    sshd
  end

  it "returns a channel" do
    MallCop.interactive "127.0.0.1", username, :password => password do |c|
      assert MallCop::Channel === c
    end
  end

  it "runs commands" do
    MallCop.interactive "127.0.0.1", username, :password => password do |c|
      c.write "cd #{root}\n"
      c.write "pwd\n"
      assert_equal root, c.read.strip
    end
  end

end