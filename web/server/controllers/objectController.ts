import { NextFunction, Request, Response } from "express";
import { DtObjects } from "./../models/dtobject";
import { NewObject } from "./../models/insertObject";
import { login } from "./../models/login";
import { Objects } from "./../models/object";
import { ServerLogger } from "./../serverlog/logger";
import { ValidateObjectRequest } from "./validate/validateObjectsRequest";

const object = new DtObjects();
const validate = new ValidateObjectRequest();
const logger = new ServerLogger();
export class ObjectControllers {

    constructor() {
        // empty
    }

    public getObjectList(req: Request, res: Response, next: NextFunction) {
        // logger.addInfo("Called get all objects");
        new Objects().getObjectList(req, res);
    }

    public searchObject(req: Request, res: Response, next: NextFunction) {
        logger.addInfo("Called search objects");
        new Objects().searchObject(req, res);
    }

    public insertObject(req: Request, res: Response, next: NextFunction) {
        new NewObject().insertObject(req, res);
        logger.addInfo("Called search objects");
    }

    public insertObjectImage(req: Request, res: Response, next: NextFunction) {
        logger.addInfo("Called search object image");
        new NewObject().insertObjectImage(req, res);
    }

    public uploadImages() {
        logger.addInfo("Called upload image");
        return new NewObject().uploadImage();
    }

    public getObjectById(req: Request, res: Response, next: NextFunction) {
        logger.addInfo("Called get object by id");
        if (validate.checkGetObjectById(req, res, next)) {
            object.getObjectById(req, res);
        }
    }

    public editObjectById(req: Request, res: Response, next: NextFunction) {
        // logger.addInfo("Called update object by id");
        if (validate.checkEditObjectById(req, res, next)) {
            login.isLogin(req, res, object.editObjectById);
        }
    }

    public deleteObjectById(req: Request, res: Response, next: NextFunction) {
        // logger.addInfo("Called delete object by id");
        if (validate.checkDeleteObjectById(req, res, next)) {
            login.isLogin(req, res, object.deleteObjectById);
        }
    }

    public mergeObjects(req: Request, res: Response, next: NextFunction) {
        // logger.addInfo("Called merge objects");
        if (validate.checkMargeObjects(req, res, next)) {
            login.isLogin(req, res, object.mergeObjects);
        }
    }
}
