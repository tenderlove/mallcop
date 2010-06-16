module MallCop
  class Shell
    attr_reader :channel

    def initialize(session)
      @session = session
      @channel = session.open_channel

      @channel.shell
    end

    def sh(cmd)
      res, code = exec_with_status(cmd)
      res
    end

  private

    def exec_with_status(cmd)
      # An `echo ''` is appended for now, mostly because I don't know of
      # a better way to do this. I'll probably end up echoing a random
      # string to indicate that the command is done.
      cmd = %[#{cmd.strip} ; echo "--==LOL[[ $? ]]==--"\n]

      @channel.write(cmd)
      result = @channel.read.strip

      unless result =~ /--==LOL\[\[ (\d+) \]\]==--\Z/m
        puts "WTF: #{result.inspect}"
        raise "Something went wrong"
      end

      return $`.strip, $1.to_i
    end

  end
end