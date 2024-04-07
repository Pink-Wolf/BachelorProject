import ApiMenu from "@/lib/api/ApiMenu";
import { getOverview } from "@/lib/api/Entity";
import { Fragment } from "react";

export default async function ApiLayout({ children }) {
    const overview = await getOverview()
    return (
        <Fragment>
            <ApiMenu data={overview} />
            {children}
        </Fragment>
    );
}
