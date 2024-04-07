import { getHeader, getOverview } from "@/lib/api/Entity";
import HeaderViewer from "@/lib/api/HeaderViewer";

export default async function HeaderApi({ params }) {
    const overview = await getOverview()
    const data = {
        ...(await getHeader(params.project, params.header)),
        entities: overview
            .projects.find((x) => { return x.name === params.project })
            .headers.find((x) => { return x.name === params.header })
            .entities
    }

    return <HeaderViewer data={data} />
}
