import Testing
@testable import NUNControlBackendCore
@Test func validConfig()throws{let c=try AppConfig(serviceName:"test",version:"1",hostname:"127.0.0.1",port:8080);#expect(c.port == 8080)}
@Test func badPortFails(){#expect(throws:BackendConfigurationError.invalidValue("PORT")){try AppConfig(serviceName:"test",version:"1",hostname:"localhost",port:70000)}}
@Test func environmentLoads()throws{let c=try AppConfig.load(environment:["PORT":"9090"],defaultServiceName:"test");#expect(c.port == 9090)}
