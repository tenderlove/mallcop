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

    it "can close the channel" do
      shell do |s|
        s.channel.close
        assert_raises_with_msg(ChannelError, "The channel has been closed") do
          s.sh "echo 'hello'"
        end
      end
    end
  end
end