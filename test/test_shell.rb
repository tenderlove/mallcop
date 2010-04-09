require "helper"

module MallCop
  describe "shell" do

    before do
      sshd
    end

    def shell(&blk)
      MallCop.shell "127.0.0.1", username, :password => password, :port => 9391, &blk
    end

    it "runs commands" do
      shell do |s|
        s.sh "cd #{root}"
        assert_equal root, s.sh("pwd")
      end
    end

    it "raises the correct exception when an error occurs" do
      Session.any_instance.stubs(:native_open_channel).returns(nil)
      Session.any_instance.stubs(:native_last_errmsg).returns("Something failed")
      assert_raises_with_msg(ChannelError, "Something failed") do
        shell { }
      end
    end
  end
end