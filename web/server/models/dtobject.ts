// tslint:disable-next-line:ordered-imports
import * as bodyParser from "body-parser";
import { NextFunction, Request, Response } from "express";

import { APP_RESPONSES } from "../appResponse";
import { ServerLogger } from "./../serverlog/logger";
import { DataBase } from "./database";
import * as database from "./database";
import { db } from "./database";

const logger: ServerLogger = new ServerLogger();
export class DtObjects {
    private count: number;

    private nfStatus = APP_RESPONSES.development.not_found.httpCode;
    private nfMessage = APP_RESPONSES.development.not_found.body.message;
    private dbStatus = APP_RESPONSES.development.internal_error.httpCode;
    private dbMessage = APP_RESPONSES.development.internal_error.body.message;

    constructor() {
        // this.count = new Number();
    }

    public getObjectById(req: Request, res: Response) {
        const okStatus = APP_RESPONSES.development.internal_error.httpCode;
        db.any("select * from image inner join traffic on image.traffic_id=traffic.id inner join cameras on \
            image.cameras_id=cameras.id and image.id = $1", req.params.id)
            .then((data) => {
                if (!data.length) {
                    logger.addWarning("Get object by id = " + req.params.id + " Bad request");
                    res.status(this.nfStatus).json({error: this.nfMessage});
                    return;
                }
                logger.addInfo("Get object by id = " + req.params.id + " object get");
                res.status(okStatus).json(JSON.stringify(data));
            })
            .catch((error) => {
                logger.addError("Get object by id = " + req.params.id + " internal server error");
                res.status(this.dbStatus).json({error: this.dbMessage});
            });
    }

    public getObjectsCount() {
        db.any("select count(id) from image")
        .then((data) => {
            const dataString = JSON.stringify(data);
            const dataJson = JSON.parse(dataString);
            this.count = Number(dataJson[0].count);
        })
        .catch((error) => {
            // empty
        });
    }
    public deleteObjectById(req: Request, res: Response) {
        const delStatus = APP_RESPONSES.development.deleted.httpCode;
        const delMessage = APP_RESPONSES.development.deleted.body.message;
        db.query("delete from traffic where id = $1", req.params.id)
            .then((data) => {
                logger.addInfo("Delete object by id = " + req.params.id + " object deleted");
                res.status(delStatus).json({message: delMessage});
            })
            .catch((error) => {
                logger.addError("Delete object by id = " + req.params.id + " internal server error");
                res.status(this.dbStatus).json({error: this.dbMessage});
            });
    }

    public editObjectById(req: Request, res: Response) {
        const editStatus = APP_RESPONSES.development.edited.httpCode;
        const editMessage = APP_RESPONSES.development.edited.body.message;
        db.query ("update  traffic set  type= $2, first_detected_date= timestamp $3 where id = $1",
            [req.params.id, req.body.type, req.body.first_detected_date])
            .then((data) => {
                logger.addInfo("Update object by id = " + req.params.id + " object updated");
                res.status(editStatus).json({message: editMessage});
            })
            .catch((error) => {
                logger.addInfo("--------");
                logger.addError("Update object by id = " + req.params.id + " with new params " + req.body + " internal\
                    server error");
                res.status(this.dbStatus).json({error: this.dbMessage});
            });
    }

    public mergeObjects(req: Request, res: Response) {
        const mergeStatus = APP_RESPONSES.development.merged.httpCode;
        const mergeMessage = APP_RESPONSES.development.merged.body.message;
        const objects = req.body.object;
        let mergedObjects = "(";
        for (let i = 1; i < objects.length - 1; ++i) {
            mergedObjects += objects[i] + ",";
        }
        mergedObjects += objects[objects.length - 1];
        mergedObjects += ")";
        const query = "update image set traffic_id=" + objects[0] + " where traffic_id in " + mergedObjects + " ;";
        const delQuery = "delete from traffic where id in " + mergedObjects;
        logger.addInfo(query);
        logger.addInfo(delQuery);
        db.query (query)
            .then((data) => {
                 db.query(delQuery)
                .then((newData) => {
                    logger.addInfo("Merge objects by ids = " + objects + " objects merged");
                    res.status(mergeStatus).json({message: mergeMessage});
                })
                .catch((error) => {
                    logger.addError("Merge  objects delete objects with id in range " + mergedObjects + " internal\
                        server error");
                    res.status(this.dbStatus).json({error: this.dbMessage});
                });
            })
            .catch((error) => {
                logger.addError("Merge object by id = " + objects[0] + " with id in \
                    range " + mergedObjects + " internal server error");
                res.status(this.dbStatus).json({error: this.dbMessage});
            });
    }
}
