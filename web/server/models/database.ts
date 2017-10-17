import { IDatabase, IMain } from "pg-promise";
import * as pgPromise from "pg-promise";
import { devProd } from "../config";
import { ServerLogger } from "./../serverlog/logger";

const logger: ServerLogger = new ServerLogger();

export class DataBase {
    private db: IDatabase<any>;

    constructor() {
        logger.addInfo("db constructor called");
        this.initDevOrProd();
    }

    public getDB() {
        return this.db;
    }

    private initDevOrProd() {
        const pgp: IMain = pgPromise ({ /* Initialization Options*/ });
        const env = process.env.NODE_ENV ? process.env.NODE_ENV : "development";
        this.db = pgp(devProd[env].db);
        this.db.connect()
            .then((data) => {
                logger.addInfo("Data Base connection success ...");
            })
            .catch((err) => {
                logger.addInfo("Data base connection failure !!!");
            });
    }

}

const database: any = new DataBase();
export let db = database.getDB();
