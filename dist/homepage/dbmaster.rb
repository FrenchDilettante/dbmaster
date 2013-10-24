require 'rubygems'

require 'json'
require "net/http"
require 'sinatra'
require "uri"

get '/' do
  erb :index
end

get "/latest" do
  uri = URI.parse "https://api.github.com/repos/manudwarf/dbmaster/tags"
  http = Net::HTTP.new uri.host, uri.port
  http.use_ssl = true

  response = http.get uri.request_uri

  tagList = JSON.parse response.body

  versionRegex = /v[\d\.]+/

  tagList.each do |tag|
    puts versionRegex.match tag["name"]
  end

  "yolo"
end
