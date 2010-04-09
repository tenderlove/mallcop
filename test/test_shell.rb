require "helper"

describe "shell" do

  before do
    sshd
  end

  it "runs commands" do
    MallCop.interactive "127.0.0.1", username, :password => password, :port => 9391 do |s|
      s.sh "cd #{root}"
      assert_equal root, s.sh("pwd")
    end
  end

end