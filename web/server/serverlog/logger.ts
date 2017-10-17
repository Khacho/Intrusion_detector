import { Logger, LoggerInstance, transports} from "winston";

export class ServerLogger {
  private logger: LoggerInstance;
  private logDir = "ServerLogs";
  constructor() {
    this.logger = new (Logger)({
      transports: [
        // new (transports.Console)(),
        new (transports.File)({
          datePattern: "yyyy-MM-dd.",
          filename: "server.log",
          level: process.env.ENV === "development" ? "debug" : "info",
          prepend: true,
        }),
      ],
    });
    // this.logger.info("info");
  }
  public addInfo(info: string) {
    this.logger.info(info);
  }

  public addWarning(warning: string) {
    this.logger.warn(warning);
  }

  public addError(debugInfo: string) {
    this.logger.debug(debugInfo);
  }
}
