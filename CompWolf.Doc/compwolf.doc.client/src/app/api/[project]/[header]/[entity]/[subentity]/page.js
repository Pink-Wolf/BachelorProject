import { getEntity } from "@/lib/api/Entity";
import EntityViewer from "@/lib/api/EntityViewer";

export default async function SubEntityPage({ params }) {
    let project = decodeURIComponent(params.project)
    let header = decodeURIComponent(params.header)
    let entity = decodeURIComponent(params.entity)
    let subentity = decodeURIComponent(params.subentity)

    const owner = await getEntity(project, header, entity)
    const target = subentity

    var data = undefined
    if (target == entity) {
        data = { ...owner.constructor, type: "function" }
    } else owner.memberGroups?.find((i) => {
        i.items.find((j) => {
            if (j.name === target) data = j

            return (data !== undefined)
        })
        return (data !== undefined)
    })

    data = {
        ...data,
        project: owner.project,
        header: owner.header,
        owner: owner.name,
        namespace: owner.namespace,
        name: target,
    }

    return <EntityViewer data={data} />
}
