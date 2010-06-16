require 'digest/sha1'

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
      separator = generate_separator
      # An `echo ''` is appended for now, mostly because I don't know of
      # a better way to do this.
      cmd = cmd.dup.strip
      cmd << ";" if cmd !~ /[;&]$/
      cmd << %[ echo #{separator} $?\n]

      @channel.write(cmd)

      res = ""
      until res =~ /#{separator} (\d+)\Z/m
        res << @channel.read
      end

      return $`.strip, $1.to_i
    end

    def generate_separator
      s = Digest::SHA1.hexdigest([channel.object_id, object_id, Time.now.to_i, Time.now.usec, rand(0xFFFFFFFF)].join(":"))
      s << Digest::SHA1.hexdigest(s)
    end

  end
end