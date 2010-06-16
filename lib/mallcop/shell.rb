module MallCop
  class Shell
    attr_reader :channel

    def initialize(session)
      @session = session
      @channel = session.open_channel

      @channel.shell
    end

    def sh(cmd)
      # An `echo ''` is appended for now, mostly because I don't know of
      # a better way to do this. I'll probably end up echoing a random
      # string to indicate that the command is done.
      cmd = "#{cmd.strip}\necho ''\n"
      @channel.write(cmd)
      @channel.read.strip
    end
  end
end