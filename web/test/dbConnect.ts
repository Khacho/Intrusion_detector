import { IDatabase, IMain } from 'pg-promise';
import * as pgPromise from 'pg-promise';

const db: IDatabase<any> =  pgPromise({}) ({
            'host': 'localhost',
            'port': 5432,
            'database': 'project',
            'user': 'postgres',
            'password': 'root'
        });

export { db }
