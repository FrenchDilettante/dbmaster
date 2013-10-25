require 'rubygems'

require 'json'
require 'net/http'
require 'sinatra'
require 'sinatra/memcache'
require 'uri'

get '/' do
  @notifyVersion = false
  unless params[:version].nil?
    @latestVersion = latest
    @notifyVersion = (@latestVersion != params[:version])
  end
  
  erb :index
end

def latest
  uri = URI.parse "https://api.github.com/repos/manudwarf/dbmaster/tags"
  http = Net::HTTP.new uri.host, uri.port
  http.use_ssl = true

  response = http.get uri.request_uri

  tagList = JSON.parse response.body

  versionRegex = /\Av([\d|\.]+)\z/

  latest = nil
  i = 0
  begin
    latest = versionRegex.match tagList[i]["name"]
    i += 1
  end while latest.nil?

  latest[1]
end
