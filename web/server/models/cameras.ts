// tslint:disable-next-line:ordered-imports
import * as bodyParser from "body-parser";
import { NextFunction, Request, Response } from "express";
import { APP_RESPONSES } from "../appResponse";
import { ServerLogger } from "./../serverlog/logger";
import { db } from "./database";

const logger: ServerLogger = new ServerLogger();

export class Cameras {

    private nfStatus = APP_RESPONSES.development.not_found.httpCode;
    private nfMessage = APP_RESPONSES.development.not_found.body.message;
    private dbStatus = APP_RESPONSES.development.internal_error.httpCode;
    private dbMessage = APP_RESPONSES.development.internal_error.body.message;

    constructor() {
        // empty
    }

    public getCameras(req: Request, res: Response) {
        db.query ("SELECT * FROM cameras")
        .then((data) => {
            if (!data.length) {
                logger.addWarning("Get cameras not found");
                res.status(this.nfStatus).json({error: this.nfMessage});
                return;
            }
            res.send(JSON.stringify(data));
        })
        .catch((error) => {
            logger.addError("Get cameras internal server error");
            res.status(this.dbStatus).json({error: this.dbMessage});
        });
    }

    public getCameraById(req: Request, res: Response) {
        const okStatus = APP_RESPONSES.development.internal_error.httpCode;
        if (req.params.id < 0) {
            logger.addWarning("Get camera by id = " + req.params.id + " Bad request");
            res.status(400).json({message: "Bad request."});
        } else {
        db.query ("SELECT * FROM cameras WHERE id=$1", req.params.id)
            .then((data) => {
                if (!data.length) {
                    logger.addWarning("Get camera by id = " + req.params.id + " not found");
                    res.status(this.nfStatus).json({error: this.nfMessage});
                    return;
                }
                logger.addInfo("Get camera by id = " + req.params.id + " camera geted");
                res.status(okStatus).json(JSON.stringify(data));
            })
            .catch((error) => {
                logger.addError("Get camera by id = " + req.params.id + " internal server error");
                res.status(this.dbStatus).json({error: this.dbMessage});
            });
       }
   }
    public deleteCameraById(req: Request, res: Response) {
        const delStatus = APP_RESPONSES.development.deleted.httpCode;
        const delMessage = APP_RESPONSES.development.deleted.body.message;
        if (req.params.id < 0) {
            logger.addWarning("Delete camera by id = " + req.params.id + " Bad request");
            res.status(400).json({message: "Bad request."});
        } else {
            db.query ("DELETE FROM cameras WHERE id=$1", req.params.id)
            .then((data) => {
                logger.addInfo("Delete camera by id = " + req.params.id + " camera deleted");
                res.status(delStatus).json({message: delMessage});
            })
            .catch((error) => {
                logger.addError("Delete camera by id = " + req.params.id + " internal server error");
                res.status(this.dbStatus).json({error: this.dbMessage});
            });
        }
    }

    public updateCameraById(req: Request, res: Response) {
        const editStatus = APP_RESPONSES.development.edited.httpCode;
        const editMessage = APP_RESPONSES.development.edited.body.message;
        const obj  = {
            description: req.body.description,
            id : req.params.id,
            location: "(" + req.body.longitude + "," + req.body.latitude + ")",
            name: req.body.cameraName,
            online: req.body.online,
        };
        db.query ("UPDATE cameras SET camera_name=${name}, location = ${location}, online=${online}, \
            description = ${description} WHERE id = ${id};", obj)
        .then((data) => {
            logger.addInfo("Update camera by id = " + req.params.id + " Update deleted");
            res.status(editStatus).json({message: editMessage});
        })
        .catch((error) => {
            logger.addError("Update camera by id = " + req.params.id + " with params " + obj + " internal\
                server error");
            res.status(this.dbStatus).json({error: this.dbMessage});
        });
    }

    public getCameraByNameOrLocation(req: Request, res: Response) {
        db.query ("SELECT * FROM cameras WHERE camera_name=$1 or (location[0] between $2-10 and $2+10 and location[1]\
            between $3-10 and $3+10)", [req.query.name, req.query.lat, req.query.long])
            .then((data) => {
                if (!data.length) {
                    logger.addWarning("Search with params " + req.query + " not faund");
                    res.status(404).json({message: "The requested resource was not found at the URL given."});
                    return;
                }
                logger.addInfo("Search camera by params " + req.query);
                res.send(JSON.stringify(data));
            })
            .catch((error) => {
                logger.addError("Search camera with params = " + req.query + " internal server error");
                res.status(500).json({message: "An unknown internal error occurred"});
            });
    }
}
